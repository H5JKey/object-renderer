from core.interfaces import AbstractFileRepository
from models import File
from schemas.file import FileCreate
from sqlalchemy import delete, select
from sqlalchemy.ext.asyncio import AsyncSession


class FileRepository(AbstractFileRepository):
    def __init__(self, session: AsyncSession) -> None:
        self.session = session

    async def get_by_id(self, file_id: int) -> File | None:
        stmt = select(File).where(File.id == file_id)
        result = await self.session.execute(stmt)
        return result.scalar()

    async def create_file(self, user_id: int, create_file_data: FileCreate) -> File:
        file = File(
            user_id=user_id,
            **create_file_data.model_dump(),
        )
        self.session.add(file)
        await self.session.commit()
        await self.session.refresh(file)
        return file

    async def delete_by_id(self, file_id: int) -> None:
        stmt = delete(File).where(File.id == file_id)
        await self.session.execute(stmt)
        await self.session.commit()
