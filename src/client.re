open ApolloLinks;

open ApolloInMemoryCache;

/* Define type for InMemoryCache's dataIdFromObject option  */
type dataObject = {
  .
  "__typename": string,
  "id": string,
  "key": string
};

let logout = () => ();

/* Create an HTTP Link */
let httpLink =
  createHttpLink(
    ~uri=
      Printf.sprintf(
        "http://serve.onegraph.io:8082/dynamic?app_id=%s",
        Config.appId
      ),
    ~credentials="include",
    ()
  );

/* Create a Link that puts an Authorization header in context */
let contextHandler = () => {
  let headers = {
    "headers": {
      "show_beta_schema": {j|true|j}
    }
  };
  headers;
};

let authLink = createContextLink(contextHandler);

/* Create a Link that handles 401 error responses */
let errorHandler = errorResponse =>
  switch errorResponse##networkError {
  | Some(error) =>
    if (error##statusCode == 401) {
      logout();
    } else {
      ();
    }
  | None => ()
  };

let errorLink = createErrorLink(errorHandler);

let inMemoryCache =
  createInMemoryCache(
    ~dataIdFromObject=
      (obj: dataObject) =>
        if (obj##__typename === "Organization") {
          obj##key;
        } else {
          obj##id;
        },
    ()
  );

/*
   Default in memory cache
   let inMemoryCache = createInMemoryCache(());
 */
/* Create the ApolloClient */
module Instance =
  ReasonApollo.CreateClient(
    {
      let apolloClient =
        ReasonApollo.createApolloClient(
          ~cache=inMemoryCache,
          ~link=from([|authLink, errorLink, httpLink|]),
          ()
        );
    }
  );
