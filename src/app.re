[@mel.module "./img/camel-fun.jpg"] external camelFun: string = "default";

open Feed;
// open Username;

module App = {
  type step =
    | Idle /* There is no request going on, and no data to be shown. In this case, we will just show instructions to proceed with the request. */
    | Loading /* A request is currently taking place to fetch the feed. */
    | InvalidUsername /* The entered username is not a valid GitHub ID. */
    | Loaded(result(Feed.feed, string)); /* A request has finished, its result is contained inside the variant value itself. */

  type state = {
    username: string,
    step,
  };

  [@react.component]
  let make = () => {
    let (state, setState) =
      React.useState(() =>
        {
          username: "jchavarri",
          step: Idle,
        }
      );

    let fetchFeed = (username: Username.t) => {
      setState(state =>
        {
          ...state,
          step: Loading,
        }
      );
      Js.Promise.(
        Fetch.fetch(
          "https://gh-feed.vercel.app/api?user="
          ++ Username.toString(username)
          ++ "&page=1",
        )
        |> then_(Fetch.Response.text)
        |> then_(text =>
             {
               let data =
                 try(Ok(text |> Json.parseOrRaise |> Feed.feed_of_json)) {
                 | Json.Decode.DecodeError(msg) =>
                   Js.Console.error(msg);
                   Error("Failed to decode: " ++ msg);
                 };
               setState(state =>
                 {
                   ...state,
                   step: Loaded(data),
                 }
               );
             }
             |> resolve
           )
      )
      |> ignore;
    };

    React.useEffect0(() => {
      switch (Username.make(state.username)) {
      | Ok(username) => fetchFeed(username)
      | Error () =>
        Js.Exn.raiseError(
          "Initial username passed to React.useState is invalid",
        )
      };
      None;
    });

    // React.useEffect0(() => {
    //   fetchFeed(Username.state.username);
    //   None;
    // });

    switch (state.step) {
    | InvalidUsername => <div> {React.string("Invalid username")} </div>
    | Idle =>
      <div>
        {React.string(
           "Press the \"Enter\" key to confirm the username selection.",
         )}
      </div>
    | Loading => <div> {React.string("Loading...")} </div>
    | Loaded(Error(msg)) => <div> {React.string(msg)} </div>
    | Loaded(Ok(feed)) =>
      <div>
        {<div>
           <label htmlFor="username-input">
             {React.string("Username:")}
           </label>
           <Username_input
             username={state.username}
             onChange={event => {
               setState(_ =>
                 {
                   ...state,
                   username: event,
                   // step: Idle,
                 }
               )
             }}
             onEnterKeyDown={() => {
               switch (Username.make(state.username)) {
               | Ok(username) => fetchFeed(username)
               | Error () =>
                 setState(state =>
                   {
                     ...state,
                     step: InvalidUsername,
                   }
                 )
               }
             }}
           />
         </div>}
        <h1> {React.string("GitHub Feed")} </h1>
        <ul>
          {feed.entries
           |> Array.map((entry: Feed.entry) =>
                <li key={entry.id}>
                  <h2> {React.string(entry.title)} </h2>
                  {entry.links
                   |> Array.map((link: Feed.link) =>
                        <a key={link.href} href={link.href}>
                          {React.string(link.href)}
                        </a>
                      )
                   |> React.array}
                  {switch (entry.content) {
                   | None => <div />
                   | Some(content) =>
                     <div dangerouslySetInnerHTML={"__html": content} />
                   }}
                </li>
              )
           |> React.array}
        </ul>
      </div>
    };
  };
};

ReactDOM.querySelector("#root")
->(
    fun
    | Some(root_elem) => {
        let root = ReactDOM.Client.createRoot(root_elem);
        ReactDOM.Client.render(root, <App />);
      }
    | None =>
      Js.Console.error(
        "Failed to start React: couldn't find the #root element",
      )
  );
