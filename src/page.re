type action =
  | SetVideoId(string)
  | SetUrl(option(string))
  | SetGithubLoggedIn(bool)
  | SetTwitterLoggedIn(bool)
  | Fail;

type state = {
  githubAuth: OneGraphAuth.t,
  twitterAuth: OneGraphAuth.t,
  videoId: option(string),
  currentUrl: option(string),
  failed: bool,
  isGithubLoggedIn: bool,
  isTwitterLoggedIn: bool
};

let component = ReasonReact.reducerComponent("Page");

let make = (~forcedVideoId=?, _children) => {
  ...component,
  initialState: () => {
    githubAuth:
      OneGraphAuth.(
        makeConfig(
          ~service="github",
          ~appId=Config.appId,
          ~oneGraphOrigin="http://serve.onegraph.io:8082",
          ~oauthFinishPath="/src/popup.html",
          ()
        )
        |> make
      ),
    twitterAuth:
      OneGraphAuth.(
        makeConfig(
          ~service="twitter",
          ~appId=Config.appId,
          ~oneGraphOrigin="http://serve.onegraph.io:8082",
          ~oauthFinishPath="/src/popup.html",
          ()
        )
        |> make
      ),
    videoId: None,
    failed: false,
    currentUrl: None,
    isGithubLoggedIn: false,
    isTwitterLoggedIn: false
  },
  reducer: (action, state) =>
    switch action {
    | SetVideoId(videoId) =>
      ReasonReact.Update({...state, videoId: Some(videoId)})
    | SetUrl(url) =>
      switch url {
      | None => ReasonReact.Update({...state, currentUrl: None})
      | Some(url) =>
        let videoId: option(string) =
          switch (Js.String.match([%bs.re "/v=([a-zA-Z0-9-_]+)/"], url)) {
          | None => None
          | Some(matches) => Some(matches[1])
          };
        ReasonReact.Update({...state, currentUrl: Some(url), videoId});
      }
    | SetGithubLoggedIn(isLoggedIn) =>
      ReasonReact.Update({...state, isGithubLoggedIn: isLoggedIn})
    | SetTwitterLoggedIn(isLoggedIn) =>
      ReasonReact.Update({...state, isTwitterLoggedIn: isLoggedIn})
    | Fail => ReasonReact.Update({...state, failed: true})
    },
  didMount: self => {
    try (
      Chrome.Tabs.getActive(activeTab => {
        Js.log2("Active tab:", activeTab);
        self.send(SetUrl(Js.Nullable.to_opt(activeTab##url)));
      })
    ) {
    | _ =>
      switch forcedVideoId {
      | None => ()
      | Some(videoId) => self.send(SetVideoId(videoId))
      }
    };
    OneGraphAuth.isLoggedIn(self.state.twitterAuth)
    |> Js.Promise.then_((isLoggedIn: Js.boolean) => {
         Js.log2("User is logged in to twitter: ", isLoggedIn);
         let isLoggedIn = Js.to_bool(isLoggedIn);
         Js.log2("User is logged in to twitter2...: ", isLoggedIn);
         self.send(SetTwitterLoggedIn(isLoggedIn));
         switch isLoggedIn {
         | false =>
           Js.log("Forcing twitter login");
           OneGraphAuth.login(self.state.twitterAuth)
           |> Js.Promise.then_(() =>
                OneGraphAuth.isLoggedIn(self.state.twitterAuth)
                |> Js.Promise.then_((isLoggedIn: Js.boolean) => {
                     let isLoggedIn = Js.to_bool(isLoggedIn);
                     Js.log2(
                       "User is logged in to twitter after login: ",
                       isLoggedIn
                     );
                     self.send(SetTwitterLoggedIn(isLoggedIn));
                     Js.Promise.resolve(isLoggedIn);
                   })
              );
         | true => Js.Promise.resolve(isLoggedIn)
         };
       })
    |> ignore;
    OneGraphAuth.isLoggedIn(self.state.githubAuth)
    |> Js.Promise.then_((isLoggedIn: Js.boolean) => {
         let isLoggedIn = Js.to_bool(isLoggedIn);
         Js.log2("User is logged in to github: ", isLoggedIn);
         self.send(SetGithubLoggedIn(isLoggedIn));
         switch isLoggedIn {
         | false =>
           OneGraphAuth.login(self.state.githubAuth)
           |> Js.Promise.then_((isLoggedIn: bool) => {
                self.send(SetGithubLoggedIn(isLoggedIn));
                Js.Promise.resolve(isLoggedIn);
              })
         | true => Js.Promise.resolve(isLoggedIn)
         };
       })
    |> ignore;
    ReasonReact.NoUpdate;
  },
  render: ({state, send: _send}) =>
    switch (
      state.isGithubLoggedIn,
      state.isTwitterLoggedIn,
      state.failed,
      state.videoId
    ) {
    | (_, _, true, _) => Utils.s("Not on a Youtube site")
    | (_, _, _, None) =>
      Utils.s(
        "No video detected: " ++ Utils.default("unknown url", state.currentUrl)
      )
    | (false, _, _, _) => Utils.s("Please log with Github")
    /* | (_, false, _, _) => Utils.s("Please log with Twitter") */
    | (true, _, _, Some(videoId)) => <div> <Example videoId /> </div>
    }
};
