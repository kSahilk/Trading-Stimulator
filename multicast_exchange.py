#!/usr/bin/env python3
"""
Multicast synthetic exchange feed (orders only).

- Every 5 seconds emits a JSON datagram with random BUY/SELL orders.
- Price change per symbol is bounded to [-50, +50] from previous price.
- Quantity is 1..50 inclusive.
- Payload is JSON per UDP datagram (newline free, UTF-8).
"""
import json
import random
import socket
import struct
import time
from datetime import datetime, timezone
from typing import List, Dict, Any

SYMBOLS = [
    "RELIANCE",
    "TCS",
    "INFY",
    "HDFCBANK",
    "ICICIBANK",
    "SBIN",
    "LT",
    "AXISBANK",
    "ITC",
    "MARUTI",
]

MULTICAST_GROUP = "239.255.0.1"
MULTICAST_PORT = 5000
UPDATE_EVERY = 5  # seconds
PRICE_JITTER = 50  # +/- max price change per tick
MIN_QTY = 1
MAX_QTY = 50


def utc_ts() -> str:
    # timezone-aware UTC to avoid deprecation warnings
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


class SyntheticBook:
    def __init__(self, symbols: List[str], seed: int = 11):
        random.seed(seed)
        self.state: Dict[str, Dict[str, Any]] = {
            sym: {"last_price": random.uniform(100, 500)} for sym in symbols
        }
        self.symbols = symbols

    def _gen_orders(self) -> List[Dict[str, Any]]:
        count = random.randint(1, len(self.symbols))
        chosen = random.sample(self.symbols, count)
        orders = []
        for sym in chosen:
            last_price = self.state[sym]["last_price"]
            delta = random.randint(-PRICE_JITTER, PRICE_JITTER)
            price = max(1, int(round(last_price + delta)))
            qty = random.randint(MIN_QTY, MAX_QTY)
            side = random.choice(["BUY", "SELL"])
            self.state[sym]["last_price"] = price
            orders.append({"symbol": sym, "side": side, "price": price, "quantity": qty})
        return orders

    def next_orders(self):
        return self._gen_orders()


def make_socket(ttl: int = 1) -> socket.socket:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, struct.pack("b", ttl))
    return sock


def send_json(sock: socket.socket, payload: Dict[str, Any]):
    data = json.dumps(payload, separators=(",", ":")).encode("utf-8")
    sock.sendto(data, (MULTICAST_GROUP, MULTICAST_PORT))


def main():
    book = SyntheticBook(SYMBOLS)
    sock = make_socket()
    print(f"Multicasting orders on {MULTICAST_GROUP}:{MULTICAST_PORT} every {UPDATE_EVERY}s")

    while True:
        now = utc_ts()
        orders = book.next_orders()
        send_json(sock, {"type": "orders", "timestamp": now, "orders": orders})
        print(f"[{now}] sent {len(orders)} orders to {MULTICAST_GROUP}:{MULTICAST_PORT}")

        time.sleep(UPDATE_EVERY)


if __name__ == "__main__":
    main()

