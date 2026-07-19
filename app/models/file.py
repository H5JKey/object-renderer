from core.constants import FILE_BUCKET_LENGTH, FILE_KEY_LENGTH
from core.database import Base
from sqlalchemy import String
from sqlalchemy.orm import Mapped
from sqlalchemy.testing.schema import mapped_column


class File(Base):
    __tablename__ = "files"

    id: Mapped[int] = mapped_column(primary_key=True)
    bucket: Mapped[str] = mapped_column(String(FILE_BUCKET_LENGTH))
    key: Mapped[str] = mapped_column(String(FILE_KEY_LENGTH))
