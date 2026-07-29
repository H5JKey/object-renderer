from typing import cast

from core.constants import ProjectVisibility
from core.exceptions.auth import PermissionDeniedError
from core.exceptions.project import ProjectIdNotFoundError
from core.interfaces.repositories import (
    AbstractProjectRepository,
    AbstractRenderRepository,
)
from models import User
from schemas.project import (
    ProjectResponse,
    ProjectResponseList,
    ProjectWithRenderCreate,
    ProjectWithRenderResponse,
)
from schemas.render import RenderResponse

from core.interfaces.services import AbstractRenderService


class ProjectService:
    def __init__(
        self,
        project_repository: AbstractProjectRepository,
    ) -> None:
        self.project_repository = project_repository

    async def get_by_id(
        self,
        project_id: int,
        user_id: int,
    ) -> ProjectResponse:
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

        return ProjectResponse.model_validate(project)

    async def get_user_projects(
        self,
        user_id: int,
        size: int,
        page: int,
    ) -> ProjectResponseList:
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
        render_repository: AbstractRenderRepository,
        render_service: AbstractRenderService,
    ) -> ProjectWithRenderResponse:
        create_project_data = create_project.project
        create_render_data = create_project.render
        render = await render_repository.create_render(create_render_data)
        render_response = RenderResponse.model_validate(render)
        project = await self.project_repository.create_project(
            user_id=user_id,
            render_id=render_response.id,
            create_project_data=create_project_data,
        )
        await render_service.create_render(create_render_data)
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
