open Utils;

let default = (value, option) =>
  switch option {
  | None => value
  | Some(value) => value
  };

module FindOtherVideosQuery = [%graphql
  {|
  query findOtherVideos($youtubeVideoId: String!, $recentRepoCount: Int!) {
  eventil {
    presentation(youtube_id: $youtubeVideoId) {
      user {
        name
        presentations {
          event {
            name
          }
          draft {
            title
          }
          video_url
          youtubeVideo {
            statistics {
              viewCount
              likeCount
            }
          }
        }
        profile {
          twitter
          twitterTimeline {
            tweets {
              idStr
              createdAt
              text
            }
          }
          github
          gitHubUser {
            login
            email
            websiteUrl
            followers {
              totalCount
            }
            repositories(first: $recentRepoCount, orderBy: {field: PUSHED_AT, direction: DESC}) {
              nodes {
                description
                name
                url
              }
            }
          }
        }
      }
    }
  }
}
|}
];

let viewCountOfPresentation = presentation =>
  switch presentation##youtubeVideo {
  | None => 0
  | Some(youtube) =>
    switch youtube##statistics {
    | None => 0
    | Some(stats) => stats##viewCount
    }
  };

let renderPresentation = presentation => {
  let title = default("No title", presentation##draft##title);
  let eventName = default("No event name", presentation##event##name);
  <li>
    (Utils.s(eventName ++ ": "))
    (
      switch presentation##video_url {
      | None => Utils.s(title)
      | Some(videoUrl) =>
        let stats =
          switch presentation##youtubeVideo {
          | None => None
          | Some(youtube) =>
            switch youtube##statistics {
            | None => None
            | Some(stats) => Some((stats##viewCount, stats##likeCount))
            }
          };
        <span>
          <a href=videoUrl target="_blank"> (Utils.s(title)) </a>
          <span>
            (
              Utils.s(
                switch stats {
                | None => ""
                | Some((viewCount, likeCount)) =>
                  Printf.sprintf(
                    " - %d views / %d likes",
                    viewCount,
                    likeCount
                  )
                }
              )
            )
          </span>
        </span>;
      }
    )
  </li>;
};

type action =
  | Click
  | Toggle;

type state = {
  count: int,
  show: bool
};

let component = ReasonReact.reducerComponent("Core");

module Query = Client.Instance.Query;

let make = (~videoId, _children) => {
  ...component,
  initialState: () => {count: 0, show: false},
  reducer: (action, state) =>
    switch action {
    | Click => ReasonReact.Update({...state, count: state.count + 1})
    | Toggle => ReasonReact.Update({...state, show: ! state.show})
    },
  render: (_) => {
    let videosQuery =
      FindOtherVideosQuery.make(
        ~youtubeVideoId=videoId,
        ~recentRepoCount=5,
        ()
      );
    <Query query=videosQuery>
      ...(
           (response, parse) =>
             switch response {
             | Loading => <div> (Utils.s("Loading")) </div>
             | Failed(error) => <div> (Utils.s(error)) </div>
             | Loaded(rawResult) =>
               let result = parse(rawResult);
               switch result##eventil {
               | None =>
                 <div> (s("No eventil response, perhaps some error?")) </div>
               | Some(eventil) =>
                 switch eventil##presentation {
                 | None =>
                   <div> (s("No other talks found from this video")) </div>
                 | Some(presentation) =>
                   let user = presentation##user;
                   <div style=(ReactDOMRe.Style.make(~minWidth="600px", ()))>
                     <h1>
                       (Utils.s("Talks by " ++ default("No name", user##name)))
                     </h1>
                     {
                       let presentations = user##presentations;
                       <ul>
                         (
                           Array.to_list(presentations)
                           |> List.sort((a, b) =>
                                viewCountOfPresentation(b)
                                - viewCountOfPresentation(a)
                              )
                           |> Array.of_list
                           |> Array.map(renderPresentation)
                           |> ReasonReact.arrayToElement
                         )
                       </ul>;
                     }
                     {
                       let gitHubUser = user##profile##gitHubUser;
                       switch gitHubUser {
                       | None => ReasonReact.nullElement
                       | Some(githubUser) =>
                         <div>
                           <hr />
                           <h3>
                             (
                               s(
                                 "Github: "
                                 ++ githubUser##login
                                 ++ "("
                                 ++ githubUser##email
                                 ++ ") - most recently active repositories:"
                               )
                             )
                           </h3>
                           (
                             switch githubUser##repositories##nodes {
                             | None => ReasonReact.nullElement
                             | Some(nodes) =>
                               <ul>
                                 (
                                   ReasonReact.arrayToElement(
                                     Array.map(
                                       optionalNode =>
                                         <li>
                                           (
                                             switch optionalNode {
                                             | None => ReasonReact.nullElement
                                             | Some(node) =>
                                               <span>
                                                 <a
                                                   href=node##url
                                                   target="_blank">
                                                   (s(node##name))
                                                 </a>
                                                 (
                                                   switch node##description {
                                                   | None => s("")
                                                   | Some(description) =>
                                                     s(" - " ++ description)
                                                   }
                                                 )
                                               </span>
                                             }
                                           )
                                         </li>,
                                       nodes
                                     )
                                   )
                                 )
                               </ul>
                             }
                           )
                         </div>
                       };
                     }
                     {
                       let twitterTimeline = user##profile##twitterTimeline;
                       let screenName = user##profile##twitter;
                       switch twitterTimeline {
                       | None => ReasonReact.nullElement
                       | Some(timeline) =>
                         <div>
                           <hr />
                           <h3> (s("Twitter: 5 most recent tweets")) </h3>
                           <ul>
                             (
                               ReasonReact.arrayToElement(
                                 Array.map(
                                   tweet =>
                                     <li>
                                       <span>
                                         <a
                                           href=(
                                             Printf.sprintf(
                                               "https://twitter.com/%s/status/%s",
                                               default("", screenName),
                                               tweet##idStr
                                             )
                                           )
                                           target="_blank">
                                           (s(tweet##createdAt))
                                         </a>
                                         (s(tweet##text))
                                       </span>
                                     </li>,
                                   timeline##tweets
                                 )
                               )
                             )
                           </ul>
                         </div>
                       };
                     }
                   </div>;
                 }
               };
             }
         )
    </Query>;
  }
};
