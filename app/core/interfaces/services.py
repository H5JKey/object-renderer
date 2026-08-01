from abc import ABC, abstractmethod
from typing import BinaryIO

from schemas.auth import LoginRequest, RegisterRequest
from schemas.file import FileResponse
from schemas.render import RenderCreate
from schemas.token import TokenInfo


class AbstractFileUploader(ABC):
    """
    Интерфейс для сервиса загрузки файлов.
    """

    @abstractmethod
    async def upload(
        self,
        file_name: str,
        file: BinaryIO,
    ) -> FileResponse:
        """
        Метод для загрузки файла.
        """


class AbstractRenderService(ABC):
    """
    Интерфейс для рендера моделей.
    """

    @abstractmethod
    async def send_event_render_model(self, create_render_data: RenderCreate) -> None:
        """
        Метод для создания моделей.
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
