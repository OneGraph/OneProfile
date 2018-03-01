let s = ReasonReact.stringToElement;

let default = (value, option) =>
  switch option {
  | None => value
  | Some(value) => value
  };

let bind = f =>
  fun
  | Some(value) => f(value)
  | None => None;
