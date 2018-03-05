type t;

type config;

[@bs.obj]
external makeConfig :
  (
    ~appId: string,
    ~service: string,
    ~oauthFinishPath: string=?,
    ~oneGraphOrigin: string=?,
    unit
  ) =>
  config =
  "";

[@bs.new] [@bs.module "onegraph-auth"] external make : config => t = "default";

[@bs.send] external login : t => Js.Promise.t('a) = "";

[@bs.send] external logout : t => Js.Promise.t('a) = "";

[@bs.send] external isLoggedIn : t => Js.Promise.t(Js.boolean) = "";
