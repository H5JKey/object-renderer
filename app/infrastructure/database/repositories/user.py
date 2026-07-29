from core.interfaces.repositories import AbstractUserRepository
from pydantic import EmailStr
from schemas.user import UserCreate, UserUpdate
from sqlalchemy import delete, select
from sqlalchemy.ext.asyncio import AsyncSession

from infrastructure.database.models import User


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

    async def get_by_email(self, email: EmailStr) -> User | None:
        stmt = select(User).filter_by(email=email)
        result = await self.session.execute(stmt)
        return result.scalar()

    async def create_user(self, create_user_data: UserCreate) -> User:
        user = User(**create_user_data.model_dump())
        self.session.add(user)
        await self.session.commit()
        await self.session.refresh(user)
        return user

    async def update_user(
        self,
        user_id: int,
        update_user_data: UserUpdate,
    ) -> User | None:
        user = await self.get_by_id(user_id)
        for field, value in update_user_data.model_dump().items():
            setattr(user, field, value)

        self.session.add(user)
        await self.session.commit()
        await self.session.refresh(user)

        return user

    async def delete_by_id(self, user_id: int) -> None:
        stmt = delete(User).where(User.id == user_id)
        await self.session.execute(stmt)
        await self.session.commit()
