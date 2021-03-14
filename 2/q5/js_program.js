// 38, program, window, [window, arguments, i, [params]], {}, undefined, undefined, undefined


// @stack:
// =========
// window
// arguments
// i
// [params]
// [0n]
// [0]
// [1n]
// ---------
{"c":"CA1807EB","a":[7816822,5127179,5577254,6478210,4049895,4644858,1225258,1297205,9752794,2081898,4043364,9226510],"t":"0000007801000000D88A5DBDEEF73B25"}

function my_em(a, e, m) {
    let res = 1n
    while (e > 0n) {
        if (e & 1n) {
            res = res * a % m;
        }
        a = a * a % m;
        e = e >> 1n;
    }
    return res;
}

window["CA1807EB"] = async function mine(params) {
    let m = 1125899906842597n;
    let bases = [
        1661594n, 2477627n, 6878794n, 7546364n, 2991919n, 2182643n,
        7930094n, 1444861n, 5596024n, 6246873n, 8726909n, 5811773n
    ];
    let a = params['a'];
    let ret = 0n;
    let tmp = 1n;
    for (var i = 0; i < a.length; i++) {
        tmp *= my_em(bases[i], BigInt(a[i]), m);
        tmp = tmp % m;
        if (i & 1) {
            ret += tmp;
            tmp = 1n;
        }
    }
    return parseInt(ret % m);
}


while (params["a"][0] > r[5][0]) {
    r[6][0] = r[6][0] * 1661594
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[5][0] = 0
while (params["a"][1] > r[5][0]) {
    r[6][0] = r[6][0] * 2477627
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[4][0] = r[4][0] + r[6][0]
r[4][0] = r[4][0] % 1125899906842597n
r[5][0] = 0
r[6][0] = 1n

while (params["a"][2] > r[5][0]) {
    r[6][0] = r[6][0] * 6878794
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[5][0] = 0
while (params["a"][3] > r[5][0]) {
    r[6][0] = r[6][0] * 7546364
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[4][0] = r[4][0] + r[6][0]
r[4][0] = r[4][0] % 1125899906842597n
r[5][0] = 0
r[6][0] = 1n

while (params["a"][4] > r[5][0]) {
    r[6][0] = r[6][0] * 2991919
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[5][0] = 0
while (params["a"][5] > r[5][0]) {
    r[6][0] = r[6][0] * 2182643
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[4][0] = r[4][0] + r[6][0]
r[4][0] = r[4][0] % 1125899906842597n
r[5][0] = 0
r[6][0] = 1n

while (params["a"][6] > r[5][0]) {
    r[6][0] = r[6][0] * 7930094
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[5][0] = 0
while (params["a"][7] > r[5][0]) {
    r[6][0] = r[6][0] * 1444861
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[4][0] = r[4][0] + r[6][0]
r[4][0] = r[4][0] % 1125899906842597n
r[5][0] = 0
r[6][0] = 1n

while (params["a"][8] > r[5][0]) {
    r[6][0] = r[6][0] * 5596024
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[5][0] = 0
while (params["a"][9] > r[5][0]) {
    r[6][0] = r[6][0] * 6246873
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[4][0] = r[4][0] + r[6][0]
r[4][0] = r[4][0] % 1125899906842597n
r[5][0] = 0
r[6][0] = 1n

while (params["a"][10] > r[5][0]) {
    r[6][0] = r[6][0] * 8726909
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[5][0] = 0
while (params["a"][11] > r[5][0]) {
    r[6][0] = r[6][0] * 5811773
    r[6][0] = r[6][0] % 1125899906842597n
    r[5][0] = r[5][0] + 1
}
r[4][0] = r[4][0] + r[6][0]
r[4][0] = r[4][0] % 1125899906842597n

parseInt(r[4][0])