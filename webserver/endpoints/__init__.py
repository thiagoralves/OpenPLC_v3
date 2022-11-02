from flask import Blueprint
from .dashboard import blueprint as dashboardBlueprint
from .programs import blueprint as programsBlueprint
from .hardware import blueprint as hardwareBlueprint
from .modbus import blueprint as modbusBlueprint
from .monitoring import blueprint as monitoringBlueprint
from .users import blueprint as usersBlueprint
from .settings import blueprint as settingsBlueprint
from .login import blueprint as loginBlueprint

blueprint = Blueprint("runtimeApp", __name__)
blueprint.register_blueprint(dashboardBlueprint)
blueprint.register_blueprint(programsBlueprint)
blueprint.register_blueprint(hardwareBlueprint)
blueprint.register_blueprint(modbusBlueprint)
blueprint.register_blueprint(monitoringBlueprint)
blueprint.register_blueprint(usersBlueprint)
blueprint.register_blueprint(settingsBlueprint)
blueprint.register_blueprint(loginBlueprint)
