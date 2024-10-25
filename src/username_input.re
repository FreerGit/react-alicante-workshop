[@mel.module "./username_input.module.css"]
external container: string = "container";

[@react.component]
let make = (~username, ~onChange, ~onEnterKeyDown) =>
  <div className=container>
    <label htmlFor="username-input"> {React.string("Username:")} </label>
    <input
      id="username-input"
      value=username
      placeholder="Enter GitHub username"
      onChange={event => onChange(event->React.Event.Form.target##value)}
      onKeyDown={event => {
        let enterKey = 13;
        if (React.Event.Keyboard.keyCode(event) == enterKey) {
          onEnterKeyDown();
        };
      }}
    />
  </div>;
