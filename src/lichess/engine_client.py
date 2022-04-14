import ctypes

_ec_lib = ctypes.CDLL("../../libengine_client.so")

_ec_lib.get_move.restype = ctypes.c_char_p


class EngineClient:
    """
    The EngineClient wraps the C++ library in a standard Python object. Its interface is designed for playing
    a game with an opponent: it can either get a move from the current position, or update itself according
    to a move made by an opponent.
    """
    def __init__(self, fen: str = ""):
        if fen:
            self._engine_client = _ec_lib.get_client(ctypes.c_char_p(bytes(fen)))
        else:
            self._engine_client = _ec_lib.get_client()

    def register_opponent_move(self, uci: str) -> None:
        _ec_lib.register_opponent_move(self._engine_client, ctypes.c_char_p(uci.encode("utf8")))

    def get_computer_move(self, seconds: float) -> str:
        return _ec_lib.get_computer_move(self._engine_client, ctypes.c_double(seconds)).decode("utf8")