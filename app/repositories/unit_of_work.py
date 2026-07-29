from types import TracebackType
from typing import Self

from core.interfaces.clients import AbstractUnitOfWork
from core.interfaces.repositories import AbstractRepository
from sqlalchemy.ext.asyncio import AsyncSession


class UnitOfWork(AbstractUnitOfWork):
    def __init__(self, session: AsyncSession) -> None:
        self.session = session

    async def __aenter__(self) -> Self:
        """
        Метод для работы с контекстным менеджером при входе.
        """
        return self

    async def __aexit__(
        self,
        exc_type: type[BaseException] | None,
        exc_val: BaseException | None,
        exc_tb: TracebackType | None,
    ) -> None:
        """
        Метод для работы с контекстным менеджером при выходе.
        """
        if exc_type is not None:
            await self.rollback()
        await self.commit()

    def get_repository(
        self,
        repository_class: type[AbstractRepository],
    ) -> AbstractRepository:
        repository = repository_class(self.session)
        return repository

    async def rollback(self) -> None:
        await self.session.rollback()

    async def commit(self) -> None:
        await self.session.commit()
