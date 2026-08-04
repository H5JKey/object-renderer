from logging import StreamHandler, getLogger
from typing import Literal, ClassVar

from pydantic import BaseModel, Field, ConfigDict


class LoggingFormatterConfig(BaseModel):
    format: str = (
        "[%(asctime)s.%(msecs)03d] %(module)10s:%(lineno)-3d %(levelname)-7s - %(message)s"
    )
    datefmt: str = "%Y-%m-%d %H:%M:%S"


class LoggingConfig(BaseModel):
    level: Literal[
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "CRITICAL",
    ] = "INFO"
    formatter: LoggingFormatterConfig = LoggingFormatterConfig()
    base_logger_name: str = "app"
