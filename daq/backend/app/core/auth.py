import secrets
import time
from collections import defaultdict, deque

from fastapi import Header

from app.config import settings
from app.core.errors import unauthorized


_attempts: dict[str, deque[float]] = defaultdict(deque)


def _check_rate_limit(client_id: str) -> None:
    now = time.time()
    window = settings.auth_rate_limit_window_seconds
    limit = settings.auth_rate_limit_attempts
    attempts = _attempts[client_id]
    while attempts and attempts[0] < now - window:
        attempts.popleft()
    if len(attempts) >= limit:
        raise unauthorized("Too many authentication attempts")


def require_upload_password(authorization: str | None = Header(default=None)) -> None:
    client_id = "global"
    _check_rate_limit(client_id)

    token = ""
    if authorization and authorization.lower().startswith("bearer "):
        token = authorization[7:]

    if not secrets.compare_digest(token, settings.upload_password):
        _attempts[client_id].append(time.time())
        raise unauthorized("Invalid upload password")
