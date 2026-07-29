from typing import Annotated

from annotated_types import Len, MaxLen
from core.constants import (
    RENDER_HEIGHT_MAX_VALUE,
    RENDER_HEIGHT_MIN_VALUE,
    RENDER_SAMPLES_MAX_VALUE,
    RENDER_SAMPLES_MIN_VALUE,
    RENDER_WIDTH_MAX_VALUE,
    RENDER_WIDTH_MIN_VALUE,
    USER_EMAIL_MAX_LENGTH,
    USER_EMAIL_MIN_LENGTH,
    USER_ENCRYPTED_PASSWORD_MAX_LENGTH,
    USER_NAME_MAX_LENGTH,
    USER_NAME_MIN_LENGTH,
    USER_PASSWORD_MAX_LENGTH,
    USER_PASSWORD_MIN_LENGTH,
    USER_SURNAME_MAX_LENGTH,
    USER_SURNAME_MIN_LENGTH,
    USER_USERNAME_MAX_LENGTH,
    USER_USERNAME_MIN_LENGTH,
)
from pydantic import EmailStr, Field

SurnameConstraint = Annotated[
    str,
    Len(
        min_length=USER_SURNAME_MIN_LENGTH,
        max_length=USER_SURNAME_MAX_LENGTH,
    ),
]

NameConstraint = Annotated[
    str,
    Len(
        min_length=USER_NAME_MIN_LENGTH,
        max_length=USER_NAME_MAX_LENGTH,
    ),
]

UsernameConstraint = Annotated[
    str,
    Len(
        min_length=USER_USERNAME_MIN_LENGTH,
        max_length=USER_USERNAME_MAX_LENGTH,
    ),
]

EmailConstraint = Annotated[
    EmailStr,
    Len(
        min_length=USER_EMAIL_MIN_LENGTH,
        max_length=USER_EMAIL_MAX_LENGTH,
    ),
]

EncryptedPasswordConstraint = Annotated[
    str,
    MaxLen(
        max_length=USER_ENCRYPTED_PASSWORD_MAX_LENGTH,
    ),
]

PasswordConstraint = Annotated[
    str,
    Len(
        min_length=USER_PASSWORD_MIN_LENGTH,
        max_length=USER_PASSWORD_MAX_LENGTH,
    ),
]

WidthConstraint = Annotated[
    int,
    Field(
        ge=RENDER_WIDTH_MIN_VALUE,
        le=RENDER_WIDTH_MAX_VALUE,
    ),
]

HeightConstraint = Annotated[
    int,
    Field(
        ge=RENDER_HEIGHT_MIN_VALUE,
        le=RENDER_HEIGHT_MAX_VALUE,
    ),
]


SampleConstraint = Annotated[
    int,
    Field(
        ge=RENDER_SAMPLES_MIN_VALUE,
        le=RENDER_SAMPLES_MAX_VALUE,
    ),
]
