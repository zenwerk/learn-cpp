/*
 * from: https://in-neuro.hatenablog.com/entry/2020/02/20/014708o
 * 不動点コンビネータによるC++での再帰型の定義例
 */
#include <vector>
#include <variant>
#include <string>

/*
 * テンプレートエイリアスで別名 `Var<T>` を設定
 * Varそのものは非再帰定義
 * テンプレート変数`T` はコンビネータから自身を受け取るための口
 */
template<class T>
using Var = std::variant<int, bool, double, std::string, std::vector<T>>;

/*
 * CRTPパターン
 * 基底クラスから派生クラスのメンバを **直接** 呼び出せることがCRTPの特長
 *----------------------------------------------------------------
 * (F) -> テンプレート変数一つを受け取る型 `RecType` を受け取る構造体 `Fix`
 * (R) -> テンプレート変数一つを受け取る型 `RecType`
 */
template <template<typename> class RecType>
struct Fix : RecType<Fix<RecType>> // Var< Fix<Var> >となるので variant の中に std::vector<Fix<Var>> が登録される
                                   // CRTPパターンより基底のVarからFix構造体のメンバは直接呼び出せるため `using宣言` より
                                   // Fix<Var> は直接 Var のデータにアクセスできる -> これによって再帰的なデータアクセスが可能になる
{
    // 継承した R は Fix<Var> なので R<Fix<R>> は Fix<Var<Fix<Var<Fix...>>>  と再帰していく
    // using .. R -> は using Var となり, Var のメンバには std::vector<Fix<Var>> があり, Fix<Var> は R<Fix<R>> をCRTPで自己参照できるので...を繰り返す
    using RecType<Fix>::RecType;
};

using ScriptParameter = Fix<Var>;


int main()
{
    using V = std::vector<ScriptParameter>;
    ScriptParameter k {V{1, false, "abc", V{2, V{"x", "y"}, 3.0}}};
    auto a = V{1, 2, 3, ScriptParameter{1}, ScriptParameter {ScriptParameter {}}};
}