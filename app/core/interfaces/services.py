from abc import ABC, abstractmethod
from typing import BinaryIO

from schemas.auth import LoginRequest, RegisterRequest
from schemas.file import FileResponse
from schemas.token import TokenInfo


class AbstractFileUploader(ABC):
    """
    Интерфейс для сервиса загрузки файлов.
    """

    @abstractmethod
    async def upload(
        self,
        user_id: int,
        file_name: str,
        file: BinaryIO,
    ) -> FileResponse:
        """
        Метод для загрузки файла.
        """


class AbstractAuthService(ABC):
    """
    Интерфейс для сервиса аутентификации / авторизации.
    """

    @abstractmethod
    async def register(
        self,
        register_user_data: RegisterRequest,
    ) -> TokenInfo:
        """
        Регистрация пользователя.
        """

    @abstractmethod
    async def authenticate_user(
        self,
        auth_user_data: LoginRequest,
    ) -> TokenInfo:
        """
        Вход пользователя в приложение.
        """

    @abstractmethod
    async def refresh_access_token(self, user_id: int) -> TokenInfo:
        """
        Обновление токена доступа.
        """
