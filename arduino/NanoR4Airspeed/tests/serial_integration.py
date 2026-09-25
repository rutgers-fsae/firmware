"""Clearly simulated 1 kHz acquisition; all generated data stays under work/."""
import os,sys,pty,select,threading,time,queue,tempfile,csv,json
from pathlib import Path
PROJECT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(PROJECT / 'logger'))
from airspeed_serial import SerialWorker

master,slave=pty.openpty()
os.set_blocking(master,False)
port=os.ttyname(slave)
running=True
state={'rate':20,'raw':8220,'sequence':0,'micros':0}

def emulator():
    incoming=b''
    pending=b''
    due=time.monotonic()
    while running:
        readable,_,_=select.select([master],[],[],.001)
        if readable:
            try: incoming+=os.read(master,4096)
            except BlockingIOError: pass
            while b'\n' in incoming:
                command,incoming=incoming.split(b'\n',1)
                if command==b'INFO': pending+=b'HELLO,2,1000,20\n'
                elif command.startswith(b'RATE,'):
                    state['rate']=int(command.split(b',')[1]); due=time.monotonic()
                    pending+=f"RATE_OK,{state['rate']}\n".encode()
        now=time.monotonic()
        data=[]
        while due<=now and len(data)<100:
            state['sequence']+=1
            state['micros']+=1000000//state['rate']
            data.append(f"D,{state['sequence']},{state['micros']},{state['raw']},0,{state['rate']},0\n")
            due+=1/state['rate']
        if data: pending+=''.join(data).encode()
        if pending:
            try:
                sent=os.write(master,pending)
                pending=pending[sent:]
            except BlockingIOError: pass

thread=threading.Thread(target=emulator,daemon=True); thread.start()
commands,events,batches=queue.Queue(),queue.Queue(),queue.Queue(maxsize=8)
testdir=PROJECT / '.test-output' / 'serial-integration'; testdir.mkdir(parents=True, exist_ok=True)
worker=SerialWorker(testdir,commands,events,batches,port_override=port); worker.start()

def wait(kind, timeout=8, predicate=lambda x:True):
    deadline=time.monotonic()+timeout
    while time.monotonic()<deadline:
        try: item=events.get(timeout=.1)
        except queue.Empty: continue
        if item['kind'] in ('fatal','error'): raise AssertionError(item)
        if item['kind']==kind and predicate(item): return item
    raise AssertionError('Timed out waiting for '+kind)

try:
    wait('firmware')
    commands.put({'kind':'rate','rate':1000})
    wait('rate_applied',predicate=lambda x:x['rate']==1000)
    commands.put({'kind':'start','title':'SIMULATED integration 1000 Hz'})
    started=wait('recording')
    time.sleep(.2)
    commands.put({'kind':'zero'})
    wait('calibration',timeout=8,predicate=lambda x:x.get('calibration') is not None)
    state['raw']=8350; time.sleep(.4)
    state['raw']=8100; time.sleep(.2)
    commands.put({'kind':'stop'})
    wait('recording',predicate=lambda x:not x['active'])
    commands.put({'kind':'shutdown'}); worker.join(timeout=3)
    rows=list(csv.DictReader(open(started['path'])))
    samples=[r for r in rows if r['raw_counts']]
    assert len(samples)>4500,len(samples)
    assert any(r['reading_state']=='valid' and float(r['airspeed_m_s'])>0 for r in rows)
    assert any(r['reading_state']=='negative_pressure_check_tubing' and r['airspeed_m_s']=='' for r in rows)
    assert all('T' in r['received_utc'] and '+00:00' in r['received_utc'] for r in rows)
    assert any(r['calibration_id'] for r in rows)
    assert all((int(b['device_sequence'])-int(a['device_sequence'])-1)==int(b['missing_serial_samples']) for a,b in zip(samples,samples[1:]))
    assert 'simulated-integration-1000-hz' in Path(started['path']).name
    print(json.dumps({'result':'PASS','simulated_sample_rows':len(samples),'file':started['path'],'checks':['rate acknowledgement','named CSV','5-second zero','positive airspeed','negative rejection','UTC unchanged','all samples logged even when preview queue fills']}))
finally:
    running=False
    commands.put({'kind':'shutdown'}); worker.join(timeout=3)
    thread.join(timeout=2)
    os.close(master); os.close(slave)
