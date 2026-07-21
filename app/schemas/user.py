from datetime import datetime

from pydantic import BaseModel

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
