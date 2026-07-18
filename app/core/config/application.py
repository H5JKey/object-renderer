from pathlib import Path
from typing import ClassVar

from pydantic_settings import BaseSettings, SettingsConfigDict

from core.config.database import DataBaseConfig


class Settings(BaseSettings):
    base_dir: Path = Path(__file__).parent.parent.parent
    database: DataBaseConfig = DataBaseConfig()

    model_config: ClassVar[SettingsConfigDict] = SettingsConfigDict(
        case_sensitive=False,
        env_file=base_dir / ".env",
        env_nested_delimiter="__",
    )


settings = Settings()
