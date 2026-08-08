# Backend

> FastAPI-based backend for distributed rendering management

---

## 🚀 Quick Start

```bash
# Install
poetry install

# Migrate
poetry run alembic upgrade head

# Run
poetry run uvicorn main:app --reload --port 8000
```

Or with Docker:

```bash
docker-compose up -d backend
```

---

## 🔑 Key Features

- **JWT Auth** – Access + refresh tokens
- **File Upload** – MinIO/S3 storage
- **Render Jobs** – Queue via Kafka
- **Async DB** – PostgreSQL + SQLAlchemy
