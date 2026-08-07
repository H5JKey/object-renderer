from typing import cast

from core.exceptions.user import UserIdNotFoundError
from core.interfaces.repositories import AbstractUserRepository
from core.logging import get_logger
from infrastructure.database.models import User
from schemas.user import UserResponse, UserUpdate

logger = get_logger(__name__)


class UserService:
    def __init__(self, user_repository: AbstractUserRepository) -> None:
        self.user_repository = user_repository

    async def get_by_id(self, user_id: int) -> UserResponse:
        user = await self.user_repository.get_by_id(user_id)
        if user is None:
            raise UserIdNotFoundError(user_id)

        logger.info(
            "Received user, user_id=%s, username=%s",
            user_id,
            user.username,
        )
        return UserResponse.model_validate(user)

    async def update_user(
        self,
        user_id: int,
        update_user_data: UserUpdate,
    ) -> UserResponse:
        user = await self.user_repository.get_by_id(user_id)
        if user is None:
            raise UserIdNotFoundError(user_id)

        user = await self.user_repository.update_user(
            user_id=user_id,
            update_user_data=update_user_data,
        )
        user = cast(User, user)
        logger.info(
            "Updated user, user_id=%s, username=%s",
            user_id,
            user.username,
        )
        return UserResponse.model_validate(user)

    async def delete_by_id(self, user_id: int) -> None:
        user = await self.user_repository.get_by_id(user_id)
        if user is None:
            raise UserIdNotFoundError(user_id)

        logger.info(
            "Deleted user, user_id=%s, username=%s",
            user_id,
            user.username,
        )
        await self.user_repository.delete_by_id(user_id)
