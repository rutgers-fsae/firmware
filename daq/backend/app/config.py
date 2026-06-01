from pathlib import Path

from pydantic_settings import BaseSettings, SettingsConfigDict


class Settings(BaseSettings):
    model_config = SettingsConfigDict(env_file=".env", extra="ignore")

    upload_password: str = "changeme"
    data_dir: Path = Path(__file__).resolve().parents[2] / "data"
    registry_path: Path = Path(__file__).resolve().parents[1] / "storage" / "datasets.json"
    cors_origins: str = "*"
    max_preview_rows: int = 200
    max_chart_rows: int = 20000
    auth_rate_limit_window_seconds: int = 60
    auth_rate_limit_attempts: int = 8


settings = Settings()
