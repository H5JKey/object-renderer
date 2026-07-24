from core.exceptions.user import UserIdNotFoundError
from core.interfaces.repositories import AbstractUserRepository
from schemas.user import UserResponse


class UserService:
    def __init__(self, user_repository: AbstractUserRepository) -> None:
        self.user_repository = user_repository

    async def get_by_id(self, user_id: int) -> UserResponse:
        user = await self.user_repository.get_by_id(user_id)
        if user is None:
            raise UserIdNotFoundError(user_id)

        return UserResponse.model_validate(user)

    async def delete_by_id(self, user_id: int) -> None:
        user = await self.user_repository.get_by_id(user_id)
        if user is None:
            raise UserIdNotFoundError(user_id)

        await self.user_repository.delete_by_id(user_id)
