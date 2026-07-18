from datetime import UTC, datetime, timedelta

import jwt

from core.config.application import settings


def encode_jwt(
    payload: dict,
    algorithm: str = settings.jwt.algorithm,
    key: str = settings.jwt.private_key,
    expires_in_minutes: int = settings.jwt.access_token_expires_in_minutes,
) -> str:
    now_time = datetime.now(UTC)
    expire_time = now_time + timedelta(minutes=expires_in_minutes)
    payload.update(
        iat=now_time,
        exp=expire_time,
    )
    return jwt.encode(
        payload=payload,
        algorithm=algorithm,
        key=key,
    )


def decode_jwt(
    token: str,
    algorithm: str = settings.jwt.algorithm,
    key: str = settings.jwt.public_key,
) -> dict:
    return jwt.decode(
        jwt=token,
        algorithms=[algorithm],
        key=key,
    )
