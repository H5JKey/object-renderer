from typing import cast

from core.constants import ProjectVisibility
from core.exceptions.auth import PermissionDeniedError
from core.exceptions.project import ProjectIdNotFoundError
from core.interfaces.repositories import AbstractProjectRepository
from models import User
from schemas.project import (
    ProjectCreate,
    ProjectResponse,
    ProjectResponseList,
)


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
        create_project: ProjectCreate,
    ) -> ProjectResponse:
        project = await self.project_repository.create_project(
            user_id=user_id,
            create_project_data=create_project,
        )
        return ProjectResponse.model_validate(project)

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
