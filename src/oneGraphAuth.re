type t;

[@bs.new] [@bs.module "onegraph-auth"]
external make :
  {
    .
    "appId": string,
    "service": string,
    "oauthFinishPath": string
  } =>
  t =
  "default";

[@bs.send] external login : t => Js.Promise.t('a) = "";

[@bs.send] external logout : t => Js.Promise.t('a) = "";

[@bs.send] external isLoggedIn : t => Js.Promise.t(Js.boolean) = "";
