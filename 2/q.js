const assets = "http://159.75.70.9:8080";
const cgi = "http://159.75.70.9:8081";

var button = $("#plant_btn");
var count = $('#plant_count');

function show(text) {
    button.prop('disabled', true);
    button.text(text);
}

function done() {
    button.prop('disabled', false);
    button.text('点我继续种树');
}

function counter(n) {
    count.text('您已累计种下 ' + n + ' 棵树');
}

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

function run(name) {
  const url = `${assets}/${name}.js`;

  return new Promise((resolve, reject) => {
    function resolving() {
      if (window[name]) {
        return resolve(window[name]);
      } else {
        return reject();
      }
    }

    if (document.querySelector(`head > script[ src = "${url}" ]`) !== null) {
      return resolving();
    }

    const script = document.createElement("script");
    script.src = url;
    script.onload = resolving;
    script.onerror = reject;
    document.head.appendChild(script);
  });
}

async function tree(token) {
  try {
    show("准备中");
    const pull = await (await fetch(`${cgi}/pull?u=${token}`)).json();
    await sleep(500);

    if (!pull.c || !pull.t) {
      throw pull;
    }

    show("种植中");
    const val = await (await run(pull.c))(pull);

    show("浇水中");
    const push = await (await fetch(`${cgi}/push?t=${pull.t}&a=${val}`)).json();
    await sleep(500);

    if (!push.success) {
      throw push;
    }

    show("成功种下一棵树");
    counter(push.score);
    await sleep(2000);
  } catch (e) {
    alert(typeof e == "object" && e.error ? e.error : "发生了一些错误，请稍后重试。");
  }

  done();
}
