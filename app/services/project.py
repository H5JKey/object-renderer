from typing import cast

from core.constants import ProjectVisibility
from core.exceptions.auth import PermissionDeniedError
from core.exceptions.project import ProjectIdNotFoundError
from core.exceptions.user import UserIdNotFoundError
from core.interfaces.clients import AbstractUnitOfWorkClient
from core.interfaces.services import AbstractRenderService
from infrastructure.database.models import User
from infrastructure.database.repositories.project import ProjectRepository
from infrastructure.database.repositories.render import RenderRepository
from infrastructure.database.repositories.user import UserRepository
from schemas.project import (
    ProjectResponse,
    ProjectResponseList,
    ProjectWithRenderCreate,
    ProjectWithRenderResponse,
)
from schemas.render import RenderResponse


class ProjectService:
    def __init__(
        self,
        unit_of_work: AbstractUnitOfWorkClient,
    ) -> None:
        self.unit_of_work = unit_of_work
        self.user_repository = self.unit_of_work.get_repository(UserRepository)
        self.project_repository = self.unit_of_work.get_repository(ProjectRepository)
        self.render_repository = self.unit_of_work.get_repository(RenderRepository)

    async def get_by_id(
        self,
        project_id: int,
        user_id: int,
    ) -> ProjectWithRenderResponse:
        project = await self.project_repository.get_by_id(
            project_id,
        )
        if project is None:
            raise ProjectIdNotFoundError(project_id)

        get_owner_coroutine = self.project_repository.get_project_owner(
            project_id,
        )
        owner = cast(User, await get_owner_coroutine)
        if (
            project.visibility == ProjectVisibility.private.value
            and owner.id != user_id
        ):
            detail = "You are not allowed to watch this project."
            raise PermissionDeniedError(detail)

        return ProjectWithRenderResponse.model_validate(project)

    async def get_user_projects(
        self,
        user_id: int,
        size: int,
        page: int,
    ) -> ProjectResponseList:
        user = await self.user_repository.get_by_id(user_id)
        if user is None:
            raise UserIdNotFoundError(user_id)

        get_projects = self.project_repository.get_user_projects(
            user_id,
        )
        projects = [
            ProjectResponse.model_validate(project) for project in await get_projects
        ]
        return ProjectResponseList(
            project_list=projects,
            size=size,
            page=page,
        )

    async def get_user_public_projects(
        self,
        user_id: int,
        size: int,
        page: int,
    ) -> ProjectResponseList:
        user = await self.user_repository.get_by_id(user_id)
        if user is None:
            raise UserIdNotFoundError(user_id)

        get_projects = self.project_repository.get_user_public_projects(
            user_id,
        )
        projects = [
            ProjectResponse.model_validate(project) for project in await get_projects
        ]
        return ProjectResponseList(
            project_list=projects,
            size=size,
            page=page,
        )

    async def create_project(
        self,
        user_id: int,
        create_project: ProjectWithRenderCreate,
        render_service: AbstractRenderService,
    ) -> ProjectWithRenderResponse:
        create_project_data = create_project.project
        create_render_data = create_project.render
        render = await self.render_repository.create_render(create_render_data)
        project = await self.project_repository.create_project(
            user_id=user_id,
            render_id=render.id,
            create_project_data=create_project_data,
        )
        await render_service.send_event_render_model(create_render_data)
        render_response = RenderResponse.model_validate(render)
        project_response = ProjectResponse.model_validate(project)
        project_with_render_response = ProjectWithRenderResponse(
            render=render_response,
            **project_response.model_dump(),
        )
        return project_with_render_response

    async def delete_by_id(
        self,
        project_id: int,
        user_id: int,
    ) -> None:
        project = await self.project_repository.get_by_id(
            project_id,
        )
        if project is None:
            raise ProjectIdNotFoundError(project_id)

        get_owner_coroutine = self.project_repository.get_project_owner(
            project_id,
        )
        owner = cast(User, await get_owner_coroutine)
        if owner.id != user_id:
            detail = "You are not allowed to watch this project."
            raise PermissionDeniedError(detail)

        await self.project_repository.delete_by_id(project_id)
