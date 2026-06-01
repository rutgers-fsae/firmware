from datetime import datetime

from pydantic import BaseModel


class DatasetRecord(BaseModel):
    slug: str
    filename: str
    original_name: str
    title: str
    uploaded_at: datetime
    size_bytes: int


class DatasetListItem(BaseModel):
    slug: str
    title: str
    filename: str
    uploaded_at: datetime
    size_bytes: int
