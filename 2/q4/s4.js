// (module
//   (func $import0 (import "Math" "min") (param i32 i32) (result i32))
//   (func $import1 (import "Math" "max") (param i32 i32) (result i32))
//   (export "Run" (func $func2))
//   (func $func2 (param $var0 i32) (param $var1 i32) (result i32)
//     (local $var2 i32) (local $var3 i32) (local $var4 i32) (local $var5 i32) (local $var6 i32) (local $var7 i32)
//     local.get $var0
//     local.set $var2
//     local.get $var1
//     i32.const 1
//     i32.sub
//     local.tee $var4
//     if
//       loop $label1
//         local.get $var2
//         local.set $var3
//         i32.const 0
//         local.set $var6
//         i32.const 10
//         local.set $var7
//         loop $label0
//           local.get $var3
//           i32.const 10
//           i32.rem_u
//           local.set $var5
//           local.get $var3
//           i32.const 10
//           i32.div_u
//           local.set $var3
//           local.get $var5
//           local.get $var6
//           call $import1
//           local.set $var6
//           local.get $var5
//           local.get $var7
//           call $import0
//           local.set $var7
//           local.get $var3
//           i32.const 0
//           i32.gt_u
//           br_if $label0
//         end $label0
//         local.get $var2
//         local.get $var6
//         local.get $var7
//         i32.mul
//         i32.add
//         local.set $var2
//         local.get $var4
//         i32.const 1
//         i32.sub
//         local.tee $var4
//         br_if $label1
//       end $label1
//     end
//     local.get $var2
//   )
// )

function run(v0, v1) {
    let v2, v3, v4, v5, v6, v7;
    let stacks = [];
    v2 = v0;
    v4 = v1 - 1;
    if (!v4) {
        return;
    }
    stacks.push(v4);
    while (true) {
        v3 = v2;
        v6 = 0;
        v7 = 10;
        while (true) {
            v5 = v3 % 10; // unsigned int
            v3 = Math.floor(v3 / 10);
            v6 = Math.max(v5, v6);
            v7 = Math.min(v5, v7);
            if (v3 <= 0) {
                break;
            }
        }
        v2 = v6 * v7 + v2;
        v4 = v4 - 1;
        if (v4 <= 0) {
            break;
        }
    }
    return v2;
}

window.A661E542 = async (param) => {
    let v0 = param['a'][0];
    let v1 = param['a'][1];
    let v2, v3, v4, v5, v6, v7;
    v2 = v0;
    v4 = v1 - 1;
    if (!v4) {
        return;
    }
    while (true) {
        v3 = v2;
        v6 = 0;
        v7 = 10;
        while (true) {
            v5 = v3 % 10; // unsigned int
            v3 = Math.floor(v3 / 10);
            v6 = Math.max(v5, v6);
            v7 = Math.min(v5, v7);
            if (v3 <= 0) {
                break;
            }
        }
        if (v7 == 0) {
            break;
        }
        v2 = v6 * v7 + v2;
        v4 = v4 - 1;
        if (v4 <= 0) {
            break;
        }
    }
    return v2;
}