[@mel.module "./img/camel-fun.jpg"] external camelFun: string = "default";

open Feed;

module App = {
  type loadingStatus =
    | Loading
    | Loaded(result(Feed.feed, string));

  [@react.component]
  let make = () => {
    let (data, setData) = React.useState(() => Loading);
    React.useEffect0(() => {
      Js.Promise.(
        Fetch.fetch("https://gh-feed.vercel.app/api?user=jchavarri&page=1")
        |> then_(Fetch.Response.text)
        |> then_(text =>
             {
               let data =
                 try(Ok(text |> Json.parseOrRaise |> Feed.feed_of_json)) {
                 | Json.Decode.DecodeError(msg) =>
                   Js.Console.error(msg);
                   Error("Failed to decode: " ++ msg);
                 };
               setData(_ => Loaded(data));
             }
             |> resolve
           )
      )
      |> ignore;
      None;
    });

    switch (data) {
    | Loading => <div> {React.string("Loading...")} </div>
    | Loaded(Error(msg)) => <div> {React.string(msg)} </div>
    | Loaded(Ok(feed)) =>
      <div>
        <h1> {React.string("GitHub Feed")} </h1>
        <ul>
          {feed.entries
           |> Array.map((entry: Feed.entry) =>
                <li key={entry.id}>
                  <h2> {React.string(entry.title)} </h2>
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
