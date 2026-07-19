from pydantic import BaseModel


class DataBaseConfig(BaseModel):
    username: str = "username"
    password: str = "password"
    host: str = "localhost"
    port: int = 5432
    db_name: str = "renderer"

    pool_size: int = 5
    max_overflow: int = 10
    pool_timeout: int = 30
    pool_recycle: int = 3600
    echo: bool = False

    @property
    def url(self) -> str:
        return f"postgresql+asyncpg://{self.username}:{self.password}@{self.host}:{self.port}/{self.db_name}"
