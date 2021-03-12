window.A593C8B8 = async (_) => (($, _, __, ___, ____) => {
    let _____ = function*() {
        while ([]) yield [(_, __) => _ + __, (_, __) => _ - __, (_, __) => _ * __][++__ % 3]["bind"](+[], ___, ____)
    }();
    let ______ = function(_____, ______, _______) {
        ____ = _____;
        ___ = ______["next"]()["value"]();
        __ == _["a"]["length"] && _______(-___)
    };
    return new Promise(__ => _["a"]["forEach"](___ => $["setTimeout"](____ => ______(___, _____, __), ___)))
})(window, _, +[], +[], +[])


window.A593C8B8 = async (param) => (($, param, count, ___, ____) => {
    let generator = function*() {
        while (true) {
            yield [
                (a, b) => a + b,
                (a, b) => a - b,
                (a, b) => a * b
            ][++count % 3]["bind"](0, ___, ____)
        }
    }();
    let calc = function(val, gen, resolve) {
        ____ = val;
        let b = ___;
        ___ = gen["next"]()["value"]();
        console.log(b, ____, ___);
        count == param["a"]["length"] && resolve(-___)
    };
    return new Promise(resolve => param["a"]["forEach"](val => $["setTimeout"](____ => calc(val, generator, resolve), val)))
})(window, param, 0, 0, 0)


window.A593C8B8 = async function (param) {
    let values = param['a'].sort((a, b) => (a > b));
    let ret = 0;
    let funcs = [
        (a, b) => a + b,
        (a, b) => a - b,
        (a, b) => a * b
    ];
    for (var i = 0; i < values.length; i++) {
        let b = ret;
        ret = funcs[(i+1) % 3](ret, values[i]);
    }
    return -ret;
}