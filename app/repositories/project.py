from core.constants import ProjectVisibility
from core.interfaces.repositories import AbstractProjectRepository
from models import Project, User
from schemas.project import ProjectCreate
from sqlalchemy import and_, delete, select
from sqlalchemy.ext.asyncio import AsyncSession


class ProjectRepository(AbstractProjectRepository):
    def __init__(self, session: AsyncSession) -> None:
        self.session = session

    async def get_by_id(self, project_id: int) -> Project | None:
        return await self.session.get(
            entity=Project,
            ident=project_id,
        )

    async def get_project_owner(self, project_id: int) -> User | None:
        stmt = select(User).join(User.projects).where(Project.id == project_id)
        result = await self.session.execute(stmt)
        return result.scalar()

    async def get_user_projects(self, user_id: int) -> list[Project]:
        stmt = select(Project).where(Project.user_id == user_id)
        result = await self.session.execute(stmt)
        return list(result.scalars().all())

    async def get_user_public_projects(
        self,
        user_id: int,
    ) -> list[Project]:
        stmt = select(Project).filter(
            and_(
                Project.user_id == user_id,
                Project.visibility == ProjectVisibility.public.value,
            ),
        )
        result = await self.session.execute(stmt)
        return list(result.scalars().all())

    async def create_project(
        self,
        user_id: int,
        render_id: int,
        create_project_data: ProjectCreate,
    ) -> Project:
        project = Project(
            user_id=user_id,
            render_id=render_id,
            **create_project_data.model_dump(),
        )
        self.session.add(project)
        await self.session.commit()
        await self.session.refresh(project)
        return project

    async def delete_by_id(self, project_id: int) -> None:
        stmt = delete(Project).where(Project.id == project_id)
        await self.session.execute(stmt)
        await self.session.commit()
