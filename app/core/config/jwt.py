from pydantic import BaseModel


class JWTConfig(BaseModel):
    access_token_expires_in_minutes: int = 15
    refresh_token_expires_in_minutes: int = 30 * 24 * 60
    algorithm: str = "RS256"
    public_key: str = "public_key"
    private_key: str = "private_key"
