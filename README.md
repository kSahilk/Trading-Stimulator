# Trading Simulator

Lightweight C++ HFT simulator with an in‑memory order book and low‑latency
order lifecycle.  `singleOrder` is the primitive – it carries side/qty/price
and exposes zero‑copy `place`, `modify` and `cancel` operations directly on
the book (`MarketData`).

The included *auto‑freeze* strategy demonstrates a simple execution algo that
chunks a parent quantity into child orders on every market tick.  New
execution strategies can be slotted in by inheriting `StrategyBase` and
reacting to `onMarketDataUpdate()`.

An auxiliary Python process (`multicast_exchange.py`) generates a UDP multicast
feed of synthetic orders.  The C++ engine currently runs standalone but could
be hooked to consume this low‑latency stream.

---

## Features

- `singleOrder` object with embedded match logic (no external manager).
- Simulated L2 book with best‑bid/ask matching and book updates.
- Auto‑freeze child‑order algorithm as a reference execution strategy.
- Optional UDP multicast market‑data generator (Python) for external feeds.
- CMake build, designed for macOS/Linux with modern C++ compilers.

## Getting started

```sh
# create build directory and compile
mkdir -p build && cd build
cmake ..
make

# run the simulator
./TradingSimulator
```

There are no external dependencies beyond the C++ standard library; Python 3
is required only for the optional multicast script.
## Usage

On launch the CLI offers:

```
1. Create trader account
2. Submit order (manual or algo)
3. Query P&L/portfolio
0. Shutdown
```

Manual orders are entered as:

```
SYMBOL QTY PRICE SIDE
```

Algo orders (auto‑freeze) take the form:

```
SYMBOL QTY PRICE EXCHANGE SIDE
```

Fill events print to stdout; account balance and position adjustments are
stubbed in `User` and triggered after simulated confirmations.
This repo is intentionally small and modular; `singleOrder` handles
matching, with other components providing user, market‑data and strategy
support.  Feel free to extend or repurpose the code for your own experiments.

Happy coding!
