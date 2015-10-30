#pragma once

namespace TargetEnvironment
{
  enum ErrorCode
  {
    // Operation success
    OK_EC,
    // Fail to register on server. Wrong id.
    RegisterFail_EC,
    // Cant get targets list form server
    RequestFail_EC,
    // Unknown Error
    Unknown_EC
  };
}