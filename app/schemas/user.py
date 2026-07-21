from datetime import datetime
from typing import ClassVar

from pydantic import BaseModel, ConfigDict

from schemas.constraints import (
    EmailConstraint,
    EncryptedPasswordConstraint,
    NameConstraint,
    SurnameConstraint,
    UsernameConstraint,
)


class UserBase(BaseModel):
    """
    Базовая схема для пользователя.
    """

    surname: SurnameConstraint
    name: NameConstraint
    username: UsernameConstraint
    email: EmailConstraint

    model_config: ClassVar[ConfigDict] = ConfigDict(from_attributes=True)


class UserCreate(UserBase):
    """
    Модель для создания пользователя.
    """

    encrypted_password: EncryptedPasswordConstraint


class UserResponse(UserBase):
    """
    Схема для вывода информации о пользователе.
    """

    id: int
    registration_date: datetime
