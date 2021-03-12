
a_array = ["A5473788"];
count = 0x196; // 407

// (function(param1, count) {
//     var fuck = function(c) {
//         while (--c) {
//             param1['push'](param1['shift']());
//         }
//     };
//     fuck(++count);
// }(a_array, 0x196));

while (--count) {
    a_array.push(a_array.shift());
}



// var _0x23fc = function(param1, _0xe936d8) {
//     param1 = param1 - 0x0;
//     var _0x23fc5a = a_array[param1];
//     return _0x23fc5a;
// };

// window[a_array[0]] = function(param1) {
//     var res = 0x30d3f;
//     for (var i = 0x30d3f; i > 0; i--) {
//         var count = 0;
//         for (var j = 0; j < i; j++) {
//             count += param1['a'][0];
//         }
//         count % param1['a'][2] == param1['a'][1] && i < res && (res = i);
//     }
//     return res;
// };

window["A5473788"] = function(param1) {
    for (var i = 1; i <= 0x30d3f; i++) {
        var count = param1['a'][0] * i;
        if (count % param1['a'][2] == param1['a'][1]) {
            return i;
        }
    }
    return 0x30d3f;
};