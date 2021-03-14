async function tree(token) {
  try {
    while (1) {
      const pull = await (await fetch(`${cgi}/pull?u=${token}`)).json();
      if (!pull.c || !pull.t) {
        throw pull;
      }
      const val = await window[pull.c](pull);
      const push = await (await fetch(`${cgi}/push?t=${pull.t}&a=${val}`)).json();
      if (!push.success) {
        throw push;
      }
      counter(push.score);
    }
  } catch (e) {
    console.log(e);
    alert(typeof e == "object" && e.error ? e.error : "发生了一些错误，请稍后重试。");
  }
  // done();
}

function myfetch(name) {
  const url = `${assets}/${name}.js`;
  return new Promise((resolve, reject) => {
    const script = document.createElement("script");
    script.src = url;
    function resolving(evt) {
      console.log(evt.target);
      if (window[name]) {
        return resolve(evt.target.innerHTML);
      } else {
        return reject();
      }
    }
    script.onload = resolving;
    script.onerror = reject;
    document.head.appendChild(script);
  });
}
