from datetime import datetime
from typing import TYPE_CHECKING

from core.constants import (
    RENDER_PROJECT_DESCRIPTION_MAX_LENGTH,
    RENDER_PROJECT_NAME_MAX_LENGTH,
    RENDER_PROJECT_NAME_MIN_LENGTH,
    ProjectVisibility,
    RenderStatus,
)
from infrastructure.database import Base
from sqlalchemy import CheckConstraint, Enum, ForeignKey, String, func
from sqlalchemy.orm import Mapped, mapped_column, relationship

if TYPE_CHECKING:
    from models import File, User


class RenderProject(Base):
    __tablename__ = "render_project"

    id: Mapped[int] = mapped_column(primary_key=True)
    name: Mapped[str] = mapped_column(String(RENDER_PROJECT_NAME_MAX_LENGTH))
    description: Mapped[str | None] = mapped_column(
        String(RENDER_PROJECT_DESCRIPTION_MAX_LENGTH),
    )
    create_date: Mapped[datetime] = mapped_column(
        server_default=func.timezone("UTC", func.now()),
    )
    user_id: Mapped[int] = mapped_column(ForeignKey("users.id", ondelete="CASCADE"))
    source_file_id: Mapped[int] = mapped_column(
        ForeignKey("files.id", ondelete="CASCADE"),
    )
    rendered_file_id: Mapped[int | None] = mapped_column(
        ForeignKey("files.id", ondelete="CASCADE"),
    )
    status: Mapped[RenderStatus] = mapped_column(
        Enum(RenderStatus, name="render_status"),
        server_default=RenderStatus.rendering.value,
    )
    visibility: Mapped[ProjectVisibility] = mapped_column(
        Enum(ProjectVisibility, name="project_visibility"),
        server_default=ProjectVisibility.public.value,
    )

    user: Mapped["User"] = relationship(
        "User",
        back_populates="render_projects",
    )
    source_file: Mapped["File"] = relationship(
        "File",
        foreign_keys=[source_file_id],
        back_populates="project_as_source_file",
    )
    rendered_file: Mapped["File"] = relationship(
        "File",
        foreign_keys=[rendered_file_id],
        back_populates="project_as_rendered_file",
    )

    __table_args__ = (
        CheckConstraint(
            f"""
            LENGTH(name) >= {RENDER_PROJECT_NAME_MIN_LENGTH}
            AND LENGTH(name) <= {RENDER_PROJECT_NAME_MAX_LENGTH}
            """,
            name="length_render_project_name",
        ),
    )
