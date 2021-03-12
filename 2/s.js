async function mytree(token) {
  try {
    while (1) {
      const pull = await (await fetch(`${cgi}/pull?u=${token}`)).json();
      if (!pull.c || !pull.t) {
        throw pull;
      }
      const val = await (await run(pull.c))(pull);
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