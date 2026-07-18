from pydantic import BaseModel


class DataBaseConfig(BaseModel):
    username: str = "username"
    password: str = "password"
    host: str = "localhost"
    port: int = 5432
    db_name: str = "postgres"

    @property
    def url(self) -> str:
        return f"postgresql+asyncpg://{self.username}:{self.password}@{self.host}:{self.port}/{self.db_name}"
