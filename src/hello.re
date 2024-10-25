module Hello = {
  [@react.component]
  let make = () => {
    <div>
      <h1> {React.string("Hello Alicante!")} </h1>
      <h2>
        {React.string("Write maintainable React applications using OCaml")}
      </h2>
    </div>;
  };
};
