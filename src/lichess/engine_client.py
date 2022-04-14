import ctypes

_ec_lib = ctypes.CDLL("../../libengine_client.so")

_ec_lib.get_move.restype = ctypes.c_char_p


class EngineClient:

    def __init__(self, fen: str = ""):
        if fen:
            self._engine_client = _ec_lib.get_client(ctypes.c_char_p(bytes(fen)))
        else:
            self._engine_client = _ec_lib.get_client()

    def advance_position(self, uci: str) -> None:
        _ec_lib.advance_position(self._engine_client, ctypes.c_char_p(uci.encode("utf8")))

    def get_move(self, seconds: float) -> str:
        return _ec_lib.get_move(self._engine_client, ctypes.c_double(seconds)).decode("utf8")
