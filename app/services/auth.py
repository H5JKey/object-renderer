from core.constants import BEARER_TOKEN_TYPE
from core.interfaces.repositories import AbstractUserRepository
from core.interfaces.services import AbstractAuthService
from core.security.jwt_tokens.factory import create_access_token, create_token
from core.security.password_utils import (
    convert_register_to_create_user,
    validate_password,
)
from schemas.auth import LoginRequest, RegisterRequest
from schemas.token import TokenInfo
from schemas.user import UserResponse
from sqlalchemy.ext.asyncio import AsyncSession


class AuthService(AbstractAuthService):
    def __init__(
        self,
        session: AsyncSession,
        user_repository: AbstractUserRepository,
    ) -> None:
        self.session = session
        self.user_repository = user_repository

    async def register(self, register_user_data: RegisterRequest) -> TokenInfo:
        create_user_data = convert_register_to_create_user(register_user_data)
        user = await self.user_repository.create_user(create_user_data)
        user_response = UserResponse.model_validate(user)
        token = create_token(user_response)
        return token

    async def authenticate_user(
        self,
        auth_user_data: LoginRequest,
    ) -> TokenInfo:
        user = await self.user_repository.get_by_username(auth_user_data.username)
        if user is None:
            raise NotImplementedError
        password = auth_user_data.password
        encrypted_password = user.encrypted_password
        if not validate_password(
            password=password,
            encrypted_password=encrypted_password,
        ):
            raise NotImplementedError

        user_response = UserResponse.model_validate(user)
        token = create_token(user_response)
        return token

    async def refresh_access_token(self, user_id: int) -> TokenInfo:
        user = await self.user_repository.get_by_id(user_id)
        user_response = UserResponse.model_validate(user)
        access_token = create_access_token(user_response)
        token = TokenInfo(
            access_token=access_token,
            token_type=BEARER_TOKEN_TYPE,
        )
        return token
