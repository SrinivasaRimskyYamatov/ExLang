# ExLang - シンプルなC++製スクリプト言語

ExLangは、軽量インタプリタ言語です。C拡張（`.so`）で簡単に機能を拡張できます。

## 📥 クイックスタート

```bash
unzip ExLang.zip
cd ExLang
chmod +x exlang
```

## ビルド手順（ソースから）

```bash
make              # exlang 本体をビルド
make math.so      # math拡張をビルド
make io.so        # IO拡張をビルド
```

**任意の拡張ビルド**:
```bash
make myext.so     # libs/myext.cpp があれば自動ビルド
```

**手動ビルドの場合**:
```bash
# 本体
g++ -std=c++17 -Wall -fPIC -rdynamic -Wl,--export-dynamic -o exlang *.cpp -ldl -lm

# C拡張
g++ -std=c++17 -shared -fPIC -Wl,--export-dynamic -o math.so math_ext.cpp -I.
```

## 実行方法

```bash
# ファイル実行
./exlang hello.ex

# REPL（対話モード）
./exlang
```

## 言語仕様

### 基本
- **変数**: `let x = 10;`
- **再代入**: `x = 20;`
- **式**: `a + b;` （セミコロン必須）
- **演算子**: `+ - * / % == != < > <= >=`

### 制御構文
```ex
if (条件) { ... } else { ... }

while (条件) { ... }
```

### 関数
```ex
fn add(a, b) {
    return a + b;
}
add(1, 2);
```

### ブロックスコープ
```ex
{
    let x = 10;
}
```

### 拡張
```ex
import "math";
```

**組み込み**: `print()`

**注意**: すべての文は `;` で終了（ブロック除く）

## C拡張の作成方法

C拡張は `libs/` ディレクトリに置くことを推奨。

1. `libs/io_ext.cpp` や `libs/math_ext.cpp` を参考に作成

**新しく作る場合**:
```bash
cd libs
cp io_ext.cpp myext.cpp
# 編集後
cd ..
make myext.so
```

```cpp
#include "interpreter.h"
#include <cmath>

extern "C" void exlang_init(Interpreter& interp) {
    class MyFunc : public Function {
    public:
        Value call(Interpreter&, const std::vector<Value>& args) override {
            // 処理を書く
            return 42;
        }
        int arity() const override { return 0; }
    };
    interp.defineNative("myfunc", std::make_shared<MyFunc>());
}
```

2. コンパイル:
```bash
g++ -std=c++17 -shared -fPIC -Wl,--export-dynamic -o myext.so myext.cpp -I.
```

3. 使用:
```ex
import "myext";
myfunc();
```

## ファイル構成
- `exlang`          - 実行バイナリ
- `hello.ex`        - 基本サンプル
- `extension_example.ex` - C拡張サンプル
- `math_ext.cpp`    - C拡張作成例
- `Makefile`        - ビルド用
- `*.cpp / *.h`     - ソースコード

## ⚠ 注意点
- Debian / Ubuntu などでは `g++` と `libdl` が必要
- C拡張作成時は**必ず** `-Wl,--export-dynamic` を付ける
- 拡張は `./math.so` （カレントディレクトリ）を優先検索
- 浮動小数点は部分対応
- エラーメッセージは最小限

---

**Enjoy ExLang!** 質問があればいつでもどうぞ。
