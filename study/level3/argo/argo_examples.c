/*
 * ARGO - 実行例と解析の流れ
 * 
 * 例1: 単純な数値
 * 入力: "42"
 * 解析の流れ:
 * 1. parse_value() 呼び出し
 * 2. peek() で '4' を確認 → isdigit() が真
 * 3. parse_number() 呼び出し
 * 4. fscanf() で 42 を読み取り
 * 5. dst->type = INTEGER, dst->integer = 42
 * 
 * 例2: 文字列
 * 入力: "\"hello\""
 * 解析の流れ:
 * 1. parse_value() 呼び出し
 * 2. peek() で '"' を確認
 * 3. parse_string() 呼び出し
 * 4. expect('"') で開始クォートを消費
 * 5. 'h', 'e', 'l', 'l', 'o' を順次読み取り
 * 6. expect('"') で終了クォートを消費
 * 7. dst->type = STRING, dst->string = "hello"
 * 
 * 例3: エスケープ文字列
 * 入力: "\"say \\\"hello\\\"\""
 * 解析の流れ:
 * 1. parse_string() で開始クォートを消費
 * 2. 's', 'a', 'y', ' ' を読み取り
 * 3. '\\' を検出 → 次の文字 '"' を読み取り → '"' を保存
 * 4. 'h', 'e', 'l', 'l', 'o' を読み取り
 * 5. '\\' を検出 → 次の文字 '"' を読み取り → '"' を保存
 * 6. 結果: "say \"hello\""
 * 
 * 例4: 単純なオブジェクト
 * 入力: "{\"name\":\"Alice\"}"
 * 解析の流れ:
 * 1. parse_value() → peek() で '{' を確認
 * 2. parse_map() 呼び出し
 * 3. expect('{') で開始ブレースを消費
 * 4. parse_string() で "name" を解析 → key = "name"
 * 5. expect(':') でコロンを消費
 * 6. parse_value() で "Alice" を解析 → value.type = STRING, value.string = "Alice"
 * 7. ペアを保存: {key: "name", value: "Alice"}
 * 8. カンマがないので終了
 * 9. expect('}') で終了ブレースを消費
 * 
 * 例5: ネストしたオブジェクト
 * 入力: "{\"user\":{\"name\":\"Bob\",\"age\":25}}"
 * 解析の流れ:
 * 1. 外側のオブジェクト開始
 * 2. キー "user" を解析
 * 3. 値として内側のオブジェクト開始
 * 4. 内側で "name":"Bob" を解析
 * 5. カンマを消費
 * 6. 内側で "age":25 を解析
 * 7. 内側のオブジェクト終了
 * 8. 外側のオブジェクト終了
 * 
 * エラー例:
 * 入力: "{\"key\":}"
 * 1. parse_map() でキー "key" を解析
 * 2. コロンを消費
 * 3. parse_value() で '}' を検出
 * 4. どの値の種類にも該当しない
 * 5. unexpected() で "Unexpected token '}'" を出力
 * 6. 解析失敗
 */