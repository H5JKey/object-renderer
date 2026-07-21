from core.interfaces.repositories import AbstractUserRepository
from models import User
from schemas.user import UserCreate
from sqlalchemy import delete, select
from sqlalchemy.ext.asyncio import AsyncSession


class UserRepository(AbstractUserRepository):
    def __init__(self, session: AsyncSession) -> None:
        self.session = session

    async def get_by_id(self, user_id: int) -> User | None:
        return await self.session.get(
            entity=User,
            ident=user_id,
        )

    async def get_by_username(self, username: str) -> User | None:
        stmt = select(User).where(User.username == username)
        result = await self.session.execute(stmt)
        return result.scalar()

    async def create_user(self, create_user_data: UserCreate) -> User:
        user = User(**create_user_data.model_dump())
        self.session.add(user)
        await self.session.commit()
        await self.session.refresh(user)
        return user

    async def delete_by_id(self, user_id: int) -> None:
        stmt = delete(User).where(User.id == user_id)
        await self.session.execute(stmt)
        await self.session.commit()
